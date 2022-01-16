function _pge_error(msg)
    print(msg)
end

function _pge_load()
    return load()
end

function _pge_update()
    return update(PGE.timer.get_delta_time())
end
