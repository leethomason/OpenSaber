    W_INSET = 13.5 - 0.5
    D_VENT = 26.0
    dhy = math.sqrt(D_VENT*D_VENT/4 - W_INSET*W_INSET/4)

    travel(g, mat, x=cx/2 + W_INSET/2, y=cy/2+dhy)
    g.spindle('CCW', mat['spindle_speed'])
    g.move(z=0)

    g.relative()
    def path(g, plunge, total_plunge):
        g.arc2(x=-W_INSET, y=0, i=-W_INSET/2, j=-dhy,  direction='CCW')
        g.move(x=0, y=-dhy*2, z=plunge/2)
        g.arc2(x=W_INSET, y=0, i=W_INSET/2, j=dhy, direction='CCW')
        g.move(x=0, y=dhy*2, z=plunge/2)

    steps = calc_steps(CUT_DEPTH, -mat['pass_depth'])
    run_3_stages(path, g, steps)

    g.absolute()

