# Nuvoton N9H20 BitBlt Blitting Accelerator (BitBLT)
    -Scale/Rotate/Shear/Reflect function
    -Fill function
## Scaling
$$
\begin{pmatrix}
x_d \\
y_d \\
1
\end{pmatrix}
=
\begin{pmatrix}
1 & 0 & t_x \\
0 & 1 & t_y \\
0 & 0 & 1
\end{pmatrix}
\begin{pmatrix}
x_s \\
y_s \\
1
\end{pmatrix}
$$
## Rotation

## Translation

## Color Transformation

## Palette

# API Function
    -bltOpen
    -bltClose
    -bltSetTransformMatrix
    -bltGetTransformMatrix
    -bltSetSrcFormat
    -bltGetSrcFormat
    -bltSetDisplayFormat
    -bltGetDisplayFormat
    -bltEnableInt
    -bltDisableInt
    -bltIsIntEnabled
    -bltPollInt
    -bltInstallCallback
    -bltSetColorMultiplier
    -bltGetColorMultiplier
    -bltSetColorOffset
    -bltGetColorOffset
    -bltSetSrcImage
    -bltSetDestFrameBuf
    -bltSetARGBFillColor
    -bltGetARGBFillColor
    -bltGetBusyStatus
    -bltSetFillAlpha
    -bltGetFillAlpha
    -bltSetTransformFlag
    -bltGetTransformFlag
    -bltSetPaletteEndian
    -bltGetPaletteEndian
    -bltSetColorPalette
    -bltSetFillOP
    -bltGetFillOP
    -bltSetFillStyle
    -bltGetFillStyle
    -bltSetRevealAlpha
    -bltGetRevealAlpha
    -bltTrigger
    -bltSetRGB565TransparentColor
    -bltGetRGB565TransparentColor
    -bltSetRGB565TransparentCtl
    -bltGetRGB565TransparentCtl