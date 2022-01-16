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

local g = PGE.graphics

local bat_pos = 20
local bat_width = 40

local ball_x = 200
local ball_y = 200

function update(dt)
    g.clear(0, 0, 128)

    g.draw_line(10, 10, g.screen_width() - 10, 10, 255, 255, 0);
    g.draw_line(10, 10, 10, g.screen_height() - 10, 255, 255, 0);
    g.draw_line(g.screen_width() - 10, 10, g.screen_width() - 10, g.screen_height() - 10, 255, 255, 0);

    g.fill_rect(bat_pos, g.screen_height() - 20, bat_width, 10, 0, 255, 0)

    g.fill_circle(ball_x, ball_y, 5, 0, 255, 255)
end
