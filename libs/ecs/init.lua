-- adapted from https://www.lexaloffle.com/bbs/?tid=39021

local m={
    add=function (self, cmp)
        assert(cmp._name)
        self[cmp._name]=cmp
        return self
    end,
    rem=function (self, name)
        self[name]=nil
        return self
    end,
}
m.__index = m

local function ent(t)
    t=t or {}
    setmetatable(t, m)
    return t
end

local function cmp(name, t)
    t=t or {}
    t._name = name
    return t
end

local function sys(names, f)
    return function (ents, ...)
        local res={}
        for _, e in pairs(ents) do
            for _, name in pairs(names) do
                if not e[name] then goto _ end
            end
            table.insert(res, f(e, ...) or nil)
            ::_::
        end
        return res
    end
end

local function cnt(ents, cmps)
    return #sys(cmps, function () return 1 end)(ents)
end

return {
    ent=ent,
    cmp=cmp,
    sys=sys,
    cnt=cnt,
}
