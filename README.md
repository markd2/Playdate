# PlayDate

Playing around with the PlayDate SDK.

### Active Development

* [Sampler](Sampler)
* [Trek](Trek)

### Got it!

Received Satuay April 23, 2022.  Enjoying _Bloom_.  First impressions of the
device - heavier than I expected. About the expected size.  The crank feels really
nice.  The screen is a little fiddly with getting the best light.  Pixels are 
TINY.  Sometimes need to get nose-to-nose with it to see small text

Now... what to actually program it?

* [ ] Still need to flesh out the Sampler with stuffs.  Now that I can see how big/small
      things actually are.
      - [ ] Fonts - Show basic use of fonts.  
            X Word wrapping in a box.
            - Scrolling bigger text with the crank. 
            - Toggle between justified and not.  
            - May need the @nothings hash table for scrolling and wrapping out a lot of text (say for accessibility larger type)
            - Aim to integrate into TNH.  
            - Maybe have a CLI tool that can take a font, render a bunch of letters to an image and blort out a PNG and font table file.  
              - Configurable to "just numbers plus this punctation." or "all upper case" or "just !@*&!" symbols.  See if the quality is decent enough to use without retouching.  
            - Oh, also maybe do attributed strings.
            - just a COUPLE of idears...
* [ ] Timing / benchmarks for operations.  Like how is memory latency vs calculation?
      There's a ton of "use RAM not CPU" in the discord(s), but sometimes (remember the
      G5's "9th order polynomial vs one round trip to RAM" thing?  And that was over
      a decade and a half ago
* [X] TNH - the case makes a nice little stand.
      - done!  Works!  Maybe once fonts is done and we've got some
        "here's text" jazz, we can add TNH suggestions and instructions.
* [ ] Apple Trek - need apple ][ text font, and finish analyzing the 
      BASIC source. (any bets on when it'll be easier to re-write
      Integer BASIC than to understand AppleTrek? :-) )
* [ ] Papillion - butterflies, butterflies everywhere
* [ ] weird build optimizations - like compiling something readable
      with a custom tool into an in-memory binary format, using
      something like `xxd -i README.md` - prevent the necessity of
      opening a file and reading in.
* [ ] Zipball - Mega Duck (from Italian Grandma)

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
