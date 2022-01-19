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

local ball_pos = {x = 0, y = 0}
local ball_dir = {x = 0, y = 0}
local ball_speed = 20
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

    -- load the sprite
    spr_tile = g.load_sprite("./assets/tut_tiles.png")

    -- start ball
    local angle = math.random() * 2 * 3.14159
    angle = -0.4
    ball_dir.x, ball_dir.y = math.cos(angle), math.sin(angle)
    ball_pos.x, ball_pos.y = 12.5, 12.5
end

function update(dt)
    -- a better collision detection
    -- calculate where ball should be, if no collision
    local potential_ball_pos = {
        x = ball_pos.x + ball_dir.x * ball_speed * dt,
        y = ball_pos.y + ball_dir.y * ball_speed * dt
    }

    -- test for hits 4 points around ball
    local tile_ball_radial_dims = {
        x = ball_radius / block_size.w,
        y = ball_radius / block_size.h
    }

    local test_resolve_collision_point = function(point)
        local test_point = {
            x = math.floor(potential_ball_pos.x + tile_ball_radial_dims.x * point.x),
            y = math.floor(potential_ball_pos.y + tile_ball_radial_dims.y * point.y)
        }

        local tile_idx = (test_point.y - 1) * 24 + test_point.x
        local tile = blocks[tile_idx]
        if not tile or tile == 0 then
            return false
        else
            -- ball has collided with a tile
            local tile_hit = tile < 10
            if tile_hit then
                blocks[tile_idx] = tile - 1
            end

            -- collision response
            if point.x == 0 then
                ball_dir.y = ball_dir.y * -1
            end

            if point.y == 0 then
                ball_dir.x = ball_dir.x * -1
            end

            return tile_hit
        end
    end

    local has_hit_tile = false
    has_hit_tile = test_resolve_collision_point({x = 0, y = -1}) or has_hit_tile
    has_hit_tile = test_resolve_collision_point({x = 0, y = 1}) or has_hit_tile
    has_hit_tile = test_resolve_collision_point({x = -1, y = 0}) or has_hit_tile
    has_hit_tile = test_resolve_collision_point({x = 1, y = 0}) or has_hit_tile

    -- fake floor
    if ball_pos.y > 20 then
        ball_dir.y = ball_dir.y * -1
    end

    -- actually update ball position with modified direction
    ball_pos.x, ball_pos.y = ball_pos.x + ball_dir.x * ball_speed * dt, ball_pos.y + ball_dir.y * ball_speed * dt

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

    -- draw ball
    g.fill_circle(ball_pos.x * block_size.w, ball_pos.y * block_size.h, ball_radius, 0, 255, 255)
end
