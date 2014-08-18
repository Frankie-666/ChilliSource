screenWidth, screenHeight = Screen.getResolution()

sliderWidget = Widget.getInternalWidget(thisWidget, "Slider")

function onPointerDown()
    io.write("HorizontalSliderBar: Pointer down\n")
end

function onPointerMoved()
    io.write("HorizontalSliderBar: Pointer moved\n")
    --absoluteMoveBy(10, 10)
    --slider.absoluteMoveBy(currX - prevX, 0)
end

function onPointerUp()
    io.write("HorizontalSliderBar: Pointer up\n")
end

moveDir = -1
function onUpdate(in_timeSinceLastUpdate)
    x, y = Widget.getLocalAbsolutePosition(sliderWidget);
    
    if x > screenWidth * 0.5 or x < -screenWidth * 0.5 then
        moveDir = -moveDir;
    end

    Widget.absoluteMoveBy(sliderWidget, in_timeSinceLastUpdate * 100 * moveDir, 0)
end

