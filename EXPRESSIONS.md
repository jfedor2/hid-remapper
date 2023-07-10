# Expressions

_Please note: this is an experimental feature that is likely to evolve in the future._

Instead of setting a mapping input to a button or an axis and possibly adding a scaling factor, you can have HID Remapper evaluate an arbitrary arithmetic expression to determine the value to which the output of the mapping will be set. This allows for more flexibility, because the expression can be more complex than simply multiplying an input value by a factor.

HID Remapper expressions are currently input using [Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation). If you’ve heard of the Forth programming language or used an HP scientific calculator, this may sound familiar to you. In the future I would like to also allow more user-friendly forms of input.

Each expression is a list of operations. The operations all work on _the stack_. The stack is simply a list of values, which the operations can add to and remove from. Last value added to the stack is said to be on top of the stack.

Let’s consider the simplest of expressions: 2+3. In RPN syntax this would be written as:
```
2 3 add
```
Note how the operands come first and the operator comes last. Let’s walk through how HID Remapper evaluates such an expression. Initially, the stack is empty. Then we go through each element of the expression, process it, and the value that is on top of the stack after we’re done is said to be the result of the expression (which will be used as the output value of the mapping).
When we encounter the `2`, we simply add it to the stack. The stack is now \[2\]. When we encounter the `3`, we also add it to the stack. The stack is now \[2, 3\]. Things get interesting when we encounter the `add` operation. It removes the top two values from the stack (2 and 3), adds them, giving 5, and puts that result on the stack. The stack now contains one value (5). If this was the expression that we set as the input of a mapping and set the output to, say, "Cursor X", this would result in the cursor moving 5 units to the right on every iteration of the mapping engine.

Of course in the real world we’ll want the result of the expression to not always be the same, but instead depend on the state of the inputs coming from the devices connected to the HID Remapper.

Let’s consider such a real world example. Let’s say we have a gaming controller, like a PS5 DualSense, and we want to map it so that moving the left analog stick moves the mouse cursor. To make things more interesting, let’s also say that we want to add a _dead zone_, so that small noise or drift in the analog stick doesn’t move the cursor.

To achieve this we add a mapping with "Expression 1" as the input and "Cursor X" as the output (the Y or vertical axis would also need to be configured in a similar way). Now let’s think what our expression needs to look like. First we need to fetch the current value of the input. To do that we need to know the usage code that the controller uses for the left analog stick. You can use the "Monitor" tab to check this. Game controllers usually use the code 0x00010030 for the horizontal axis of the left analog stick, which also happens to be the code that mice use for the horizontal axis. To fetch the value of this input, we start our expression like this:
```
0x00010030 input_state
```
The `0x00010030` part puts that value on the stack. The `input_state` operation takes one value from the stack, fetches the input state value corresponding to that usage code, and puts it on the stack. What values does a controller send for the left analog stick? Again, this is something that you could check using the "Monitor" tab. In our case it sends values between 0 and 255, with 0 corresponding to the stick all the way to the left, 255 all the way to the right, and 128 corresponding to the stick’s neutral center position. What we would like to have is negative values for when the stick is moved to the left and positive values when it’s moved to the right, so we’ll just subtract 128 from the input value. We can do that by extending our expression like this:
```
0x00010030 input_state -128 add
```
Then let’s handle the dead zone. Let’s say that if the current value is between -10 and 10, we want to act as if it was zero, giving us around 8% dead zone. To do this we’ll use a certain trick involving Boolean logic. The expression evaluation process doesn’t have conditional logic, but it does have a `gt` operation that takes two values from the stack and puts either 1 or 0 back to the stack, depending on whether one of the values is greater than the other. This way we can know whether the input value was greater than some threshold or not. But how can we set the output to the original value if it’s bigger than the threshold and to zero if not? Let’s consider this expression and walk through it step by step:
```
0x00010030 input_state -128 add dup abs 10 gt mul
```
As we’ve discussed before, `0x00010030 input_state -128 add` fetches the input value and subtracts 128 from it. `dup` makes a copy of the top of the stack, if the stack was \[x\] before it, it is now \[x, x\]. `abs` takes a value from the stack and puts its absolute value back on the stack. So if the stack was, say \[-30, -30\], it will now be \[-30, 30\]. If it was \[5, 5\], it will still be \[5, 5\]. `10` puts the threshold value on the stack and `gt` takes two values from the stack and compares them. If the stack was \[-30, 30, 10\] before, it will now be \[-30, 1\]. If it was \[5, 5, 10\], it will now be \[5, 0\]. Now comes the trick. Even though `0` and `1` in our example corresponds to logical _false_ and _true_, there’s nothing stopping us from treating it as a number and multiplying it by the other value on the stack. That’s what the `mul` operation does. If the stack was \[-30, 1\], it will now be \[-30\]. If it was \[5, 0\], it will now be \[0\], effectively giving us the dead zone that we wanted.

Two things remain, first we should scale the output to a reasonable range. Through experimentation we find that multiplying it by 0.025 makes the cursor move with the speed we want, so we do just that:
```
0x00010030 input_state -128 add dup abs 10 gt mul 0.025 mul
```
Still with us? Let’s consider another example more briefly. Let’s say we want the D-pad on our game controller to also move the mouse cursor. We happen to know (from the "Monitor" tab) that the D-pad sends the usage code 0x00010039 with values between 0 and 7 when it’s pressed in some direction and a value outside this range (say, 8 or 15) when it’s in the neutral position. Here’s an expression that we could use for the "Cursor X" mapping:
```
0x00010039 input_state 7 gt not 0x00010039 input_state 45 mul sin mul
```
`0x00010039 input_state` fetches the input value. `7 gt` checks if it’s outside the valid range. `not` inverts that condition. If at this point we have a `1` on the stack, this means that the D-pad is being pressed in some direction. If we have a `0`, it means that it’s not. Then we fetch the input value again (we could use `dup` before the `7 gt`, but then we’d also need `swap` which we currently don’t have). The D-pad sends a 0 for North, 1 for North-East, 2 for East etc. So to get a value in degrees (0-360), we multiply the received value by 45 (`45 mul`). Trigonometry tells us that to get the horizontal axis part of the movement in a certain direction, we can use the sine function (`sin`). Now the stack is either `1` and a valid sine result or `0` and some garbage value that we’ll disregard by multiplying the two values (`mul`).

For the vertical axis the expression is almost the same, we just need to use cosine instead of sine and negate its output value:
```
0x00010039 input_state 7 gt not 0x00010039 input_state 45 mul cos -1 mul mul
```
In this case we don’t do additional scaling because we’re happy with the way it is, but we could of course do that if we wanted to.

The two examples above already show us how we can do things that weren’t previously possible with the existing mapping mechanism, but let’s go further. As you’ve noticed we can fetch the state of some input more than once in an expression. What we can also do is fetch the state of multiple different inputs and do some calculations on them to produce an output. When would we want to do that? We could for example check if some button is pressed and apply some part of the calculation only if it is, emulating the layer mechanism in a more flexible way. Or let’s say that we have a joystick, like the ones used for flight simulators, that has a stick and a throttle lever. Here’s an expression that makes the stick move the mouse cursor, but with the speed controlled by the throttle lever!
```
0x00010030 input_state -128 add dup abs 10 gt mul 0.025 mul 0x00010036 input_state -1 mul 255 add 0.007 mul mul
```
Our joystick sends the 0x00010030 usage code (range 0-255) for the X axis of the stick and the 0x00010036 usage code (also 0-255, but with 0 being the maximum speed and 255 the minimum) for the throttle lever. We fetch the state of the stick (`0x00010030 input_state`), re-center it around zero (`-128 add`), apply the dead zone (`dup abs 10 gt mul`), apply a scaling (`0.025 mul`), fetch the state of the throttle lever (`0x00010036 input_state`), invert it so that zero corresponds to the state we want to be the minimum speed (`-1 mul 255 add`), scale it (`0.007 mul`), and finally multiply the stick value by the throttle value (`mul`).

In addition to fetching the state of inputs in expressions, you can also fetch the current time. This has some interesting applications, for example you could write an expression like this to make a button work in "turbo" mode (press and release the button quickly when you hold it):
```
time 200 mod 100 gt 0x00090001 input_state_binary mul
```
`time` fetches the current time in milliseconds. `200 mod` takes that value modulo 200. `100 gt` checks if the result is in the upper half of the 200 millisecond time window, `0x00090001 input_state_binary` fetches the state of the button input and `mul` multiplies the two values, effectively pressing the button 50% of the time, with a 200 ms period.

The `time` operation could also be used to make a mouse jiggler (a function that makes the mouse move by itself). Check the "Examples" section in the web configuration tool to see what that expression might look like.

Looking for more inspiration? Try writing a set of expressions that will press one button when an analog trigger on a game controller is half-pressed and another button when it is fully pressed, like a dual stage trigger on a flight sim joystick.

## Known limitations

Currently the set of operations that you can perform in expressions is fairly limited. There’s no way to store information between subsequent evaluations of the expression and no way to access the previous state of an input. Also expressions don’t really play that well with other features like macros, tap-hold and sticky mappings. Hopefully some of these issues can be addressed in the future.

## Tips and tricks

Use `input_state` to fetch the state of relative usages like a mouse axis and non-binary absolute usages like a D-pad or an analog stick or trigger. Use `input_state_binary` to fetch the state of binary absolute usages (buttons).

The values stored on the stack are 32-bit integers that are multiplied by 1000 to simulate fractional values. Therefore it is impossible to put 0.0001 on the stack. If you want to multiply a value by 0.0001, first multiply it by 0.001 and then by 0.1.

When fetching the currently active layers with `layer_state` (and, similarly, `sticky_state`), the bitmask value is put on the stack as is, without the x1000 scaling just mentioned. Therefore if you want to perform a `bitwise_and` operation on that value, let’s say to check if layer 1 is active, you need to either use 0.002 or 0x02 as the other operand (hex values are passed as is because they’re normally used for HID usage codes). For example:
```
layer_state 0x02 bitwise_and not not 0x00090001 input_state_binary mul
```

Currently, the tap-hold and the sticky logic is only triggered for usages that are an input in a mapping with those flags set. Therefore if you want to make use of those states in an expression, you have to add another dummy mapping with the button in question set as input and `Nothing` set as output, with the appropriate tap/hold/sticky flag set.

## Operation reference

Here's a list of all operations that can be used in an expression. Each operation takes the values listed in the Input column from the stack and puts the values listed in the Output column on the stack. If multiple values are listed, the last one corresponds to the top of the stack.

| Operation | Input | Output | Notes |
| --- | --- | --- | --- |
| `12345` | | 12345 | Puts the value on the stack. |
| `0x00120034` | | 0x00120034 | Puts the value on the stack. Use for usage codes. |
| `input_state` | _usage_ | state of _usage_ input | |
| `input_state_binary` | _usage_ | state of _usage_ input | Use for buttons. |
| `prev_input_state` | _usage_ | previous state of _usage_ input | |
| `prev_input_state_binary` | _usage_ | previous state of _usage_ input | Use for buttons. |
| `add` | _x_, _y_ | _x + y_ | |
| `mul` | _x_, _y_ | _x * y_ | |
| `eq` | _x_, _y_ | _x == y_ | 1 if equal, 0 otherwise. |
| `mod` | _x_, _y_ | _x % y_ | Modulo function. |
| `gt` | _x_, _y_ | _x > y_ | 1 if x > y, 0 otherwise. |
| `not` | _x_ | _!x_ | 1 if x == 0, 0 otherwise. |
| `abs` | _x_ | _abs(x)_ | -x if x < 0, x otherwise. |
| `sin` | _x_ | _sin(x)_ | Sine function, x in degrees. |
| `cos` | _x_ | _cos(x)_ | Cosine function, x in degrees. |
| `relu`| _x_ | _relu(x)_ | 0 if x < 0, x otherwise. |
| `clamp` | _x_, _y_, _z_ | _clamp(x, y, z)_ | y if x < y, z if x > z, x otherwise. |
| `dup` | _x_ | _x_, _x_ | Duplicates top value on the stack. |
| `bitwise_or` | _x_, _y_ | _x \| y_ | |
| `bitwise_and` | _x_, _y_ | _x & y_ | |
| `bitwise_not` | _x_ | _~x_ | |
| `store` | _x_, _n_ | | Puts x in register n |
| `recall` | _n_ | register n contents | |
| `time` | | current time | In milliseconds, starting at some arbitrary point. |
| `scaling` | | _scaling_ | Value as defined in the mapping. Useful for quick parameterization. |
| `layer_state` | | _layer\_state_ | Bit mask of currently active layers. |
| `sticky_state` | _usage_ | _sticky\_state(usage)_ | Bit mask of layers on which given usage is in sticky state. |
| `tap_state` | _usage_ | _tap\_state(usage)_ | 1 if input is in tap state, 0 otherwise. |
| `hold_state` | _usage_ | _hold\_state(usage)_ | 1 if input is in hold state, 0 otherwise. |
