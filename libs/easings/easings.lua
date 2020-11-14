-- Port of https://github.com/zaklaus/raylib-go/blob/master/easings/easings.go

return {
  linearNone = function (t, b, c, d)
    return c*t/d + b
  end,

  linearIn = function (t, b, c, d)
    return c*t/d + b
  end,

  linearOut = function (t, b, c, d)
    return c*t/d + b
  end,

  linearInOut = function (t, b, c, d)
    return c*t/d + b
  end,
}
