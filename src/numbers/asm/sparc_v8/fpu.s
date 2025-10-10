.seg "text"

.global ieee_get_fcr
ieee_get_fcr:
add %sp,-120,%sp
st %fsr,[%sp+100]
ld [%sp+100],%o0
retl
sub %sp,-120,%sp


.global ieee_get_fsr
ieee_get_fsr:
add %sp,-120,%sp
st %fsr,[%sp+100]
ld [%sp+100],%o0
retl
sub %sp,-120,%sp



.global ieee_set_fcr
ieee_set_fcr:
st %o0,[%sp+68]
ld [%sp+68],%fsr
retl
nop


.global ieee_set_fsr
ieee_set_fsr:
st %o0,[%sp+68]
ld [%sp+68],%fsr
retl
nop



