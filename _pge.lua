function _pge_error(msg)
    print(msg)
end

function _pge_config()
    if config then
        PGE.config = config()
    else
        PGE.config = {
            screen_width = 256,
            screen_height = 240,
            screen_x_scale = 2,
            screen_y_scale = 2,
        }
    end
end

function _pge_load()
    load()
end

function _pge_update()
    update(PGE.timer.get_delta_time())
end
