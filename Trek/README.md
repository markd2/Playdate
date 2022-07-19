# Trek

Port of the classic Apple Trek game.

![](assets/trek.gif)

## Bringing it to Playdate

Of course, the screens between the Apple ][ and the Playdate are
quite different.  One is 40x24 characters, the other is 400x240,
which seems like it works out great, but the Apple ][ typeface 
is five points wide and 8 tall, giving 66 x 26, so doubling the
Apple font isn't going to work.

default size, things are tiny

![](assets/apple-1x.png)

double-size, things fall off the screen pretty early

![](assets/apple-2x.png)

For my bad eyes, the second one is actually readable without having to stick
my nose against the screen.

So, maybe could have a couple of panes - galaxy in one, maybe the sector
chart and current settings in another, running at 2x (or doubling the font
size)

This could be do-able
![](assets/info-panel-2x.png)

So close! Using NRG as an abbrv, it pretty much all fits except the decimal
for the stardate.  That we could drop.

![](assets/panel-nrg-2x.png)

----------

Working on a panel system (essentially views without their own hierarchy).

Here's the galaxy overview panel drawn over a 50% background:

![](assets/galaxy-panel.png)

Thinking of having info panels (galaxy / sector info) that can be moved
between with the D-pad.  Then B/A for bringing up an interactive panel (choose
the actions, program the computer)

This is what it looks like with an empty panel when pressing the B button. woo!

![](assets/overlay-panel.gif)

also added inverted drawing and moving the enterprise sector around with the
D-Pad

![](assets/moving-sector.gif)

We can't see the whole galaxy out of the gate, so have a fog of war around the ship.

![](assets/frog-of-war.gif)

----------

Time for the sector map plus status.  Hard-coding it all for now.

Next should probably come up with a good story/API for reversed
text rendering.


