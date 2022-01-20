function config()
    return {
        title = "Breakout!",
        screen_width = 512,
        screen_height = 480,
        screen_x_scale = 1,
        screen_y_scale = 1
    }
end

local g = PGE.graphics
local i = PGE.input

local bat_pos = 20
local bat_width = 40
local bat_speed = 25

local ball_pos = {x = 0, y = 0}
local ball_dir = {x = 0, y = 0}
local ball_speed = 20
local ball_radius = 5

local block_size = {w = 16, h = 16}
local blocks = {}
local spr_tile = nil
local dec_fragment = nil

local list_fragments = {}

function load()
    print("Version: " .. PGE.MajorVersion .. "." .. PGE.MinorVersion)
    print("  Screen width " .. PGE.window.screen_width())
    print("  Screen height " .. PGE.window.screen_height())

    for y = 1, 30 do
        for x = 1, 24 do
            local idx = (y - 1) * 24 + x

            if x == 1 or y == 1 or x == 24 then
                blocks[idx] = 10
            else
                blocks[idx] = 0
            end

            if x > 3 and x <= 21 and y > 4 and y <= 6 then
                blocks[idx] = 1
            elseif x > 3 and x <= 21 and y > 6 and y <= 8 then
                blocks[idx] = 2
            elseif x > 3 and x <= 21 and y > 8 and y <= 10 then
                blocks[idx] = 3
            end
        end
    end

    -- load the sprite
    spr_tile = g.load_sprite("./assets/tut_tiles.png")

    -- load fragment sprite
    local spr_frag = g.load_sprite("./assets/tut_fragment.png")
    -- create decal of fragment
    dec_fragment = g.create_decal(spr_frag)

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

    local test_resolve_collision_point = function(point, info)
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
                info.id = tile
                info.hit_pos = {
                    x = test_point.x,
                    y = test_point.y
                }
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
    local info = {}
    has_hit_tile = test_resolve_collision_point({x = 0, y = -1}, info) or has_hit_tile
    has_hit_tile = test_resolve_collision_point({x = 0, y = 1}, info) or has_hit_tile
    has_hit_tile = test_resolve_collision_point({x = -1, y = 0}, info) or has_hit_tile
    has_hit_tile = test_resolve_collision_point({x = 1, y = 0}, info) or has_hit_tile

    if has_hit_tile then
        for i = 1, 100 do
            local f = {
                pos = {
                    x = info.hit_pos.x + 0.5,
                    y = info.hit_pos.y + 0.5
                }
            }
            local angle = math.random() * 2 * 3.14159
            local velocity = math.random() * 10
            f.vel = {
                x = velocity * math.cos(angle),
                y = velocity * math.sin(angle)
            }
            f.angle = angle
            f.time = 3
            if info.id == 1 then
                f.color = {r = 255, g = 0, b = 0, a = 255}
            elseif info.id == 2 then
                f.color = {r = 0, g = 255, b = 0, a = 255}
            elseif info.id == 3 then
                f.color = {r = 255, g = 255, b = 0, a = 255}
            end

            list_fragments[#list_fragments + 1] = f
        end
    end

    -- fake floor
    if ball_pos.y > 20 then
        ball_dir.y = ball_dir.y * -1
    end

    -- actually update ball position with modified direction
    ball_pos.x, ball_pos.y = ball_pos.x + ball_dir.x * ball_speed * dt, ball_pos.y + ball_dir.y * ball_speed * dt

    local remove_idx = {}
    -- update list_fragments
    for i, f in ipairs(list_fragments) do
        f.vel.y = f.vel.y + 20 * dt
        f.pos.x, f.pos.y = f.pos.x + f.vel.x * dt, f.pos.y + f.vel.y * dt
        f.angle = f.angle + 5 * dt
        f.time = f.time - dt
        f.color.a = math.floor(f.time / 3 * 255)

        if f.time < 0 then
            remove_idx[#remove_idx + 1] = i
        end
    end

    -- remove dead fragments
    for i, idx in ipairs(remove_idx) do
        table.remove(list_fragments, idx)
    end

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

    -- draw fragments
    for i, f in ipairs(list_fragments) do
        g.draw_rotated_decal(
            f.pos.x * block_size.w,
            f.pos.y * block_size.h,
            dec_fragment,
            f.angle,
            4,
            4,
            1,
            1,
            f.color.r,
            f.color.g,
            f.color.b,
            f.color.a
        )
    end
end
