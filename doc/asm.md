# ASM instruction

## Operation

### int

iadd

isub

imul

idiv

irem

ieq

ine

ilt

igt

ile

ige

iand

ior

inot

ineg

### float

fadd

fsub

fmul

fdiv

feq

fne

flt

fgt

fle

fge

fneg

### string

sadd

seq

sne

```
v0 = opd.back()
opd.pop()
v1 = opd.back()
opd.pop()
v2 = v1 op v0
opd.push(v2)
```

## Stack

iconst imm

fconst imm

sconst imm

struct imm

```
v = Type(imm)
opd.push(v)
```

pop

## Load/Store

### global

gload gidx

```
opd.push(globals[gidx])
```

gstore gidx

```
globals[gidx] = opd.back()
opd.pop()
```

### local

lload lidx

```
opd.push(locals[lidx])
```

lstore lidx

```
locals[lidx] = opd.back()
opd.pop()
```

### field

fload fidx

```
s = opd.back()
opd.pop()
opd.push(s[fidx])
```

fstore fidx

```
v0 = opd.back()
opd.pop()
s = opd.back()
opd.pop()
s[fidx] = v0
```

### vector

vector

vappend

```
v = opd.back()
opd.pop()
s = opd.back()
s.append(v)
```

vload

```
i = opd.back()
opd.pop()
v = opd.back()
opd.pop()
opd.push(v[i])
```

vstore

```
v0 = opd.back()
opd.pop()
i = opd.back()
opd.pop()
v = opd.back()
opd.pop()
v[i] = v0
```

## Branch

br a

```
ip = a
```

brt a

```
if (opd.back() != 0)
    ip = a
```

brf a

```
if (opd.back() == 0)
    ip = a
```

## Function

call f

ret

## Utility

len

print

```
v = opd.back()
opd.pop()
opd.push(v.size())
```

halt

## Draw

drawRect

drawText
