# Kaleidoscope-ToIfAlone

This plugin enables you to have a key that acts normally when tapped, but shifts to a layer when held in conjunction with pressing other keys.
It is very similiar to [DualUse][], mainly differing by never timing out.

It is made to emulate the behaviour of Karabiner's `ToIfAlone` functionality, hence the name.

## Using the plugin

Add the plugin to your available libraries by cloning it into your `Arduino-Boards/libraries` directory.

```c++
#include <Kaleidoscope.h>
#include <Kaleidoscope-ToIfAlone.h>

// Have an enum for layers
// or some other way of assigning numbers to layers
enum {
  _BASE,
  _NUMPAD,
};

void setup(void) {
  Kaleidoscope.use(&ToIfAlone);

  Kaleidoscope.setup();

  static kaleidoscope::ToIfAlone::KeyBinding toifalonemap[] = {
    {Key_Escape, _NUMPAD},
    TOIFALONE_MAP_END
  };
  ToIfAlone.map = toifalonemap;
}
```

## Plugin methods

The plugin provides the `ToIfAlone` obkect, with the following property:

### `.map`

> Set the key map. This takes an array of `kaleidoscope::ToIfAlone::KeyBinding`
> objects.
> The last item in the array must be the special `TOIFALONE_MAP_END` value.
> Each KeyBinding objects takes the key that will be pressed and the layer that it should shift to, respectively.
>
> By default, the map is empty.

## Further reading

Starting from the [example][] is the recommended way of getting started with the plugin.

  [DualUse]: https://github.com/keyboardio/Kaleidoscope-DualUse
  [example]: https://github.com/jamesnvc/Kaleidoscope-ToIfAlone/blob/master/examples/ToIfAlone/ToIfAlone.ino
