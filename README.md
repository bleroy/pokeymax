# This repo is somewhat obsolete

I created this repo when Foft first released his design of the PokeyMax 4 board publicly.
I did so at the time because the design was still in Eagle format and somewhat difficult to keep track of.
I took the design, imported it to KiCad and published it here as a git repo.
I also took a stab at redesigning the board to bring the price down as much as possible (that's the 4s version of the board).

A lot has happened since.
Foft released multiple revisions of the board and HDL files, and quite a few people have built their own boards.
He's now using KiCad, so no more Eagle.
He also started working on a 4.5 revision as well as on several new designs, including the PokeyMin that uses a smaller FPGA version and resistors instead of level shifters.
That last design pretty much makes 4s obsolete and little more than a curiosity. For me, it was a nice exercise in designing such a small board.

In other words, nobody except maybe me should use the files on this repo. Instead, head over to Foft's site and download the latest files from there:
http://www.64kib.com/atarixlfpga_svn/trunk/atari_800xl/atari_chips/hardware/

# PokeyMax 4

Foft's public designs for PokeyMax.

Pokeys are getting rare and expensive. FPGA alternatives exist but are also expensive. Foft, the author of PokeyMax, has very generously made his designs public on [the AtariAge forums](https://forums.atariage.com/topic/364870-pokeymax-v4-bring-up-thread/).
With his permission, I'm publishing his latest iteration on PokeyMax, version 4, which is a 2-layer version with all components on the top side. This should be easier to produce at a lower cost than the previous versions. I've also imported the project in KiCad from the original Eagle files.

Foft has not specified a license for his work, but attribution should be maintained. This is his design, reproduced here under his permission.
