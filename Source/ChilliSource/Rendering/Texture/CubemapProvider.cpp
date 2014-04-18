//
//  CubemapProvider.cpp
//  Chilli Source
//  Created by Scott Downie on 17/04/2014.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Tag Games Limited
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include <ChilliSource/Rendering/Texture/CubemapProvider.h>

#include <ChilliSource/Core/Base/Application.h>
#include <ChilliSource/Core/Image/Image.h>
#include <ChilliSource/Core/Threading/TaskScheduler.h>
#include <ChilliSource/Rendering/Texture/Cubemap.h>

#include <array>

namespace ChilliSource
{
	namespace Rendering
	{
        CS_DEFINE_NAMEDTYPE(CubemapProvider);
      
        //-------------------------------------------------------
        //-------------------------------------------------------
        CubemapProviderUPtr CubemapProvider::Create()
        {
            return CubemapProviderUPtr(new CubemapProvider());
        }
		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		bool CubemapProvider::IsA(Core::InterfaceIDType in_interfaceId) const
		{
			return in_interfaceId == ResourceProvider::InterfaceID || in_interfaceId == CubemapProvider::InterfaceID;
		}
        //----------------------------------------------------------------------------
        //----------------------------------------------------------------------------
        void CubemapProvider::OnInit()
        {
            std::vector<Core::ResourceProvider*> resourceProviders;
            Core::Application::Get()->GetSystems(resourceProviders);
            
            for(u32 i=0; i<resourceProviders.size(); ++i)
            {
                if(resourceProviders[i]->GetResourceType() == Core::Image::InterfaceID)
                {
                    m_imageProviders.push_back(resourceProviders[i]);
                }
            }
        }
        //----------------------------------------------------------------------------
        //----------------------------------------------------------------------------
        Core::InterfaceIDType CubemapProvider::GetResourceType() const
        {
            return Cubemap::InterfaceID;
        }
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		bool CubemapProvider::CanCreateResourceWithFileExtension(const std::string& in_extension) const
		{
            for(u32 i=0; i<m_imageProviders.size(); ++i)
            {
                if(m_imageProviders[i]->CanCreateResourceWithFileExtension(in_extension))
                {
                    return true;
                }
            }
            
			return false;
		}
        //----------------------------------------------------------------------------
        //----------------------------------------------------------------------------
        void CubemapProvider::CreateResourceFromFile(Core::StorageLocation in_location, const std::string& in_filePath, Core::ResourceSPtr& out_resource)
		{
            LoadCubemap(in_location, in_filePath, nullptr, out_resource);
		}
        //----------------------------------------------------------------------------
        //----------------------------------------------------------------------------
        void CubemapProvider::CreateResourceFromFileAsync(Core::StorageLocation in_location, const std::string& in_filePath, const Core::ResourceProvider::AsyncLoadDelegate& in_delegate, Core::ResourceSPtr& out_resource)
        {
            Core::Task<Core::StorageLocation, const std::string&, const Core::ResourceProvider::AsyncLoadDelegate&, Core::ResourceSPtr&>
            task(this, &CubemapProvider::LoadCubemap, in_location, in_filePath, in_delegate, out_resource);
            Core::TaskScheduler::ScheduleTask(task);
        }
        //----------------------------------------------------------------------------
        //----------------------------------------------------------------------------
        void CubemapProvider::LoadCubemap(Core::StorageLocation in_location, const std::string& in_filePath, const Core::ResourceProvider::AsyncLoadDelegate& in_delegate, Core::ResourceSPtr& out_resource)
        {
            std::string fileName;
            std::string fileExtension;
            Core::StringUtils::SplitBaseFilename(in_filePath, fileName, fileExtension);
            
            Core::ResourceProvider* imageProvider = nullptr;
            for(u32 i=0; i<m_imageProviders.size(); ++i)
            {
                if(m_imageProviders[i]->CanCreateResourceWithFileExtension(fileExtension))
                {
                    imageProvider = m_imageProviders[i];
                    break;
                }
            }
            
            if(imageProvider == nullptr)
            {
                CS_LOG_ERROR("Cannot find provider for " + in_filePath);
                out_resource->SetLoadState(Core::Resource::LoadState::k_failed);
                if(in_delegate != nullptr)
                {
                    Core::Task<Core::ResourceSPtr&> task(in_delegate, out_resource);
                    Core::TaskScheduler::ScheduleMainThreadTask(task);
                }
                return;
            }
            
            const u32 k_numFaces = 6;
			//MSVC does not support moving arrays of unique_ptr at this time and therfore we have
			//to create a shared pointer in order to pass it into the lambda
			auto imageDataContainer = std::make_shared<std::array<Texture::TextureDataUPtr, k_numFaces>>();
            std::array<Texture::Descriptor, k_numFaces> descs;
            
            for(u32 i=0; i<k_numFaces; ++i)
            {
                std::string facePath = fileName + Core::ToString(i) + "." + fileExtension;
                Core::ResourceSPtr imageResource(Core::Image::Create());
                imageProvider->CreateResourceFromFile(in_location, facePath, imageResource);
                Core::ImageSPtr image(std::static_pointer_cast<Core::Image>(imageResource));
                
                if(image->GetLoadState() == Core::Resource::LoadState::k_failed)
                {
                    CS_LOG_ERROR("Failed to load cubemap face image " + facePath);
                    out_resource->SetLoadState(Core::Resource::LoadState::k_failed);
                    if(in_delegate != nullptr)
                    {
                        Core::Task<Core::ResourceSPtr&> task(in_delegate, out_resource);
                        Core::TaskScheduler::ScheduleMainThreadTask(task);
                    }
                    return;
                }
                
				(*imageDataContainer)[i] = image->MoveData();
                
                Texture::Descriptor desc;
                desc.m_width = image->GetWidth();
                desc.m_height = image->GetHeight();
                desc.m_format = image->GetFormat();
                desc.m_compression = image->GetCompression();
                desc.m_dataSize = image->GetDataSize();
                
                descs[i] = std::move(desc);
            }
            
            if(in_delegate == nullptr)
            {
                Cubemap* cubemap = (Cubemap*)out_resource.get();
				cubemap->Build(descs, *imageDataContainer);
                out_resource->SetLoadState(Core::Resource::LoadState::k_loaded);
            }
            else
            {
				Core::Task<> task([descs, imageDataContainer, in_delegate, out_resource]()
                {
                    Cubemap* cubemap = (Cubemap*)out_resource.get();
					cubemap->Build(descs, *imageDataContainer);
                    out_resource->SetLoadState(Core::Resource::LoadState::k_loaded);
                    in_delegate(out_resource);
                });
                Core::TaskScheduler::ScheduleMainThreadTask(task);
            }
        }
        //----------------------------------------------------------------------------
        //----------------------------------------------------------------------------
        void CubemapProvider::OnDestroy()
        {
            m_imageProviders.clear();
            m_imageProviders.shrink_to_fit();
        }
	}
}
