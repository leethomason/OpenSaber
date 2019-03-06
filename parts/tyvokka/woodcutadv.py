
H_WOOD = 8.0
H_STOCK = 19.3
MAT = 'np883-pine-3.175'


init(MAT, X_CAPSULE_NOM, D_CAPSULE)
flatten_stock(H_STOCK, H_WOOD)      # this is the new zero

hill(D_OUTER)

at(x=NOMINAL/2):
    hole(-H_HEAD, d=D_HEAD)
    hole("bottom", d=D_BOLT)

at(x=NOMINAL/2 - DZ_CENTER):
    hole("bottom", d=D_POWER)

at(x=NOMINAL/2 + DZ_CENTER):
    hole("bottom", d=SWITCH_D)
    hole(SWITCH_INSET_DEPTH, d=D_SWITCH_INSET)

capsule("bottom", X_CAPSULE_NOM, D_CAPSULE, "outside", True, 'x')