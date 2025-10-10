.seg "text"

.global ieee_get_fcr
ieee_get_fcr:
st %fsr,[%sp+2035]
ld [%sp+2035],%o0
retl
nop


.global ieee_get_fsr
ieee_get_fsr:
st %fsr,[%sp+2035]
ld [%sp+2035],%o0
retl
nop



.global ieee_set_fcr
ieee_set_fcr:
st %o0,[%sp+2035]
ld [%sp+2035],%fsr
retl
nop


.global ieee_set_fsr
ieee_set_fsr:
st %o0,[%sp+2035]
ld [%sp+2035],%fsr
retl
nop



