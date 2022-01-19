function config()
    return {
        title = "Breakout!",
        screen_width = 512,
        screen_height = 480,
        screen_x_scale = 1,
        screen_y_scale = 1,
        target_frame = 60
    }
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

local block_size = {w = 16, h = 16}
local blocks = {}
local spr_tile = nil

function load()
    print("Version: " .. PGE.MajorVersion .. "." .. PGE.MinorVersion)

    for y = 1, 30 do
        for x = 1, 24 do
            if x == 1 or y == 1 or x == 24 then
                blocks[(y - 1) * 24 + x] = 10
            else
                blocks[(y - 1) * 24 + x] = 0
            end

            if x > 3 and x <= 21 and y > 4 and y <= 6 then
                blocks[(y - 1) * 24 + x] = 1
            elseif x > 3 and x <= 21 and y > 6 and y <= 8 then
                blocks[(y - 1) * 24 + x] = 2
            elseif x > 3 and x <= 21 and y > 8 and y <= 10 then
                blocks[(y - 1) * 24 + x] = 3
            end
        end
    end

    spr_tile = g.load_sprite("./assets/tut_tiles.png")
end

function update(dt)
    -- erase previous frame
    g.clear(0, 0, 128)
    g.set_pixel_mode(g.PixelMode.Mask)

    for y = 1, 30 do
        for x = 1, 24 do
            local v = blocks[(y - 1) * 24 + x]
            if v == 10 then
                g.draw_partial_sprite(
                    x * block_size.w,
                    y * block_size.h,
                    spr_tile,
                    0 * block_size.w,
                    0 * block_size.h,
                    block_size.w,
                    block_size.h
                )
            elseif v == 1 then
                g.draw_partial_sprite(
                    x * block_size.w,
                    y * block_size.h,
                    spr_tile,
                    1 * block_size.w,
                    0 * block_size.h,
                    block_size.w,
                    block_size.h
                )
            elseif v == 2 then
                g.draw_partial_sprite(
                    x * block_size.w,
                    y * block_size.h,
                    spr_tile,
                    2 * block_size.w,
                    0 * block_size.h,
                    block_size.w,
                    block_size.h
                )
            elseif v == 3 then
                g.draw_partial_sprite(
                    x * block_size.w,
                    y * block_size.h,
                    spr_tile,
                    3 * block_size.w,
                    0 * block_size.h,
                    block_size.w,
                    block_size.h
                )
            end
        end
    end

    g.set_pixel_mode(g.PixelMode.Normal)
end
