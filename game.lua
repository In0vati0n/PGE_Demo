function config()
    return {
        screen_width = 512,
        screen_height = 480,
        screen_x_scale = 1,
        screen_y_scale = 1,
    }
end

function load()
    print("Version: " .. PGE.MajorVersion .. "." .. PGE.MinorVersion)
end

function update(dt)
    PGE.graphics.clear(0, 0, 128)
end
