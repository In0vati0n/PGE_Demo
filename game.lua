function config()
    return {
        screen_width = 512,
        screen_height = 480,
        screen_x_scale = 1,
        screen_y_scale = 1
    }
end

function load()
    print("Version: " .. PGE.MajorVersion .. "." .. PGE.MinorVersion)
end

local g = PGE.graphics
local i = PGE.input

local bat_pos = 20
local bat_width = 40
local bat_speed = 0.1

local ball_x = 200
local ball_y = 200
local ball_radius = 5

function update(dt)
    -- handle user input
    if i.is_key_held(i.Key.LEFT) then
        bat_pos = bat_pos - bat_speed
    end

    if i.is_key_held(i.Key.RIGHT) then
        bat_pos = bat_pos + bat_speed
    end

    if bat_pos < 11 then
        bat_pos = 11
    end

    if bat_pos + bat_width > g.screen_width() - 10 then
        bat_pos = g.screen_width() - 10 - bat_width
    end

    -- cheat! moving the ball with mouse
    if i.is_mouse_held(0) then
        ball_x = i.mouse_x()
        ball_y = i.mouse_y()
    end

    if i.mouse_wheel() > 0 then
        ball_radius = ball_radius + 1
    elseif i.mouse_wheel() < 0 then
        ball_radius = ball_radius - 1
    end

    if ball_radius < 5 then
        ball_radius = 5
    end

    g.clear(0, 0, 128)

    g.draw_line(10, 10, g.screen_width() - 10, 10, 255, 255, 0)
    g.draw_line(10, 10, 10, g.screen_height() - 10, 255, 255, 0)
    g.draw_line(g.screen_width() - 10, 10, g.screen_width() - 10, g.screen_height() - 10, 255, 255, 0)

    g.fill_rect(bat_pos, g.screen_height() - 20, bat_width, 10, 0, 255, 0)

    g.fill_circle(ball_x, ball_y, ball_radius, 0, 255, 255)
end
