function config()
    return {
        screen_width = 512,
        screen_height = 480,
        screen_x_scale = 1,
        screen_y_scale = 1,
        target_frame = 60
    }
end

function load()
    print("Version: " .. PGE.MajorVersion .. "." .. PGE.MinorVersion)
end

local g = PGE.graphics
local i = PGE.input

local bat_pos = 20
local bat_width = 40
local bat_speed = 250

local ball_x = 200
local ball_y = 200
local ball_vx = 200
local ball_vy = -100
local ball_radius = 5

function update(dt)
    -- handle user input
    if i.is_key_held(i.Key.LEFT) then
        bat_pos = bat_pos - bat_speed * dt
    end

    if i.is_key_held(i.Key.RIGHT) then
        bat_pos = bat_pos + bat_speed * dt
    end

    if bat_pos < 11 then
        bat_pos = 11
    end

    if bat_pos + bat_width > g.screen_width() - 10 then
        bat_pos = g.screen_width() - 10 - bat_width
    end

    -- update ball
    ball_x = ball_x + ball_vx * dt
    ball_y = ball_y + ball_vy * dt

    -- really crude arena detection - this approach sucks
    if ball_y <= 10 then
        ball_vy = ball_vy * -1
    end
    if ball_x <= 10 then
        ball_vx = ball_vx * -1
    end
    if ball_x >= g.screen_width() - 10 then
        ball_vx = ball_vx * -1
    end

    -- check bat
    if ball_y > g.screen_height() - 20 and ball_x > bat_pos and ball_x < bat_pos + bat_width then
        ball_vy = ball_vy * -1
    end

    -- check if ball has gone off screen
    if ball_y > g.screen_height() then
        -- reset ball location
        ball_x = 200
        ball_y = 200

        -- choose random direction
        local angle = math.random() * 2 * 3.14159
        ball_vx = 300 * math.cos(angle)
        ball_vy = 300 * math.sin(angle)
    end

    -- erase previous frame
    g.clear(0, 0, 128)

    -- draw boundary
    g.draw_line(10, 10, g.screen_width() - 10, 10, 255, 255, 0)
    g.draw_line(10, 10, 10, g.screen_height() - 10, 255, 255, 0)
    g.draw_line(g.screen_width() - 10, 10, g.screen_width() - 10, g.screen_height() - 10, 255, 255, 0)

    -- draw bat
    g.fill_rect(bat_pos, g.screen_height() - 20, bat_width, 10, 0, 255, 0)

    -- draw ball
    g.fill_circle(ball_x, ball_y, ball_radius, 0, 255, 255)
end
