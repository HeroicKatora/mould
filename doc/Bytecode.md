Opcodes and interpretation
----------------------

```
Operations
+- Insert
|  +- Index
|  |  *CodeValue
|  |
|  +- Format
|     *ImmediateValue
|
+- Literal
|  +I Pointer
|  |
|  +I Length
|
+- Stop


CodeValue (*)
+- Auto
+C ReadCodepoint


ImmediateValue (*)
+- Auto
+I ReadImmediate


Format (*)
+- Kind
|  +- 14 values
|
+- Width
|  *InlineValue
|
+- Precision
|  *InlineValue
|
+- Padding
|  *InlineValue
|
+- Alignment
|  +- Default
|  +- Left
|  +- Right
|  +- Center
|
+- Sign
   +- Default
   +- Always
   +- Pad

InlineValue (*)
+- Auto
+I Immediate
+S Inline
+A Argument
```
