function _pge_error(msg)
    print(msg)
end

function _pge_config()
    if config then
        return config()
    else
        return {
            title = "PGE",
            screen_width = 256,
            screen_height = 240,
            screen_x_scale = 2,
            screen_y_scale = 2
        }
    end
end

function _pge_load()
    load()
end

function _pge_update()
    update(PGE.timer.get_delta_time())
end

function _pge_on_destroy()
    if on_destroy then
        on_destroy()
    end
end
