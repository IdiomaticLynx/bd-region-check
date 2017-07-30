---
title: Frequently asked questions
---

# How do I use this tool?

In order to make use of this tool, you need a BD drive attached to your system as well as a BD disc or two.
Insert the BD disc in the drive and open the disc using a file browser/explorer. There should be a file named `MovieObjects.bdmv` in the folder `BDMV` on the disc.
Either drag and drop this file into the tool or click the file upload and select the file manually.

After a short time the result will show on the screen. The verdict can be one of:
 * Uncertain => can mean almost anything, but one thing is sure it does _something_ with either the region code or the country code.
 * Invalid file => the selected file does not appear to be valid, either a wrong file, corrupt file or a newer version than supported.
 * Region free => the disc never access the region or country code and is therefore region free.
 * Geo-locked => the disc does not access the region code, but does access the country code, probably geo-locking, although it could just be localization
 * Locked => both region code and country code are used in some way (assuming it's locked)

Additional information can be seen in the "Checked regions" and "Checked countries" boxes. Do note that these don't mean that it will (or won't) work with those regions or countries.
All it indicates is that the disc checks against these values, so depending on the code it could still be a whitelist or a blacklist.

# How does this tool work?

**Simple answer**
The tool scans the `MovieObjects.bdmv` file for instructions that check the region or country code.

**Long answer**
In contrast to DVDs the drive no longer plays a role in the region detection/locking. It's now fully up to the player, which means that if a region check is performed it has to be somewher in the instructions on the disc.
The tool simply runs over the instructions and checks if the region or country code player specific registers are accessed.

This does mean that the tool can easily mis-identify certain situations. But in most cases it will work reasonably well.


# Is this tool legal?

I am not a lawyer, but I highly doubt the working of this tool could get anyone in trouble. This tool does probably less then you'd expect:

 * The file in question is _never_ uploaded to the internet, everything happens inside the user's browser. So the user isn't spreading/sharing any copyrighted material.
 * The MovieObjects.bdmv file is not encrypted on the BD disc, so there is _no_ encryption/DRM scheme/etc to 'break' or circumvent .
 * _No_ region-locking is circumvented as the instructions from the file are never "excecuted" only statically analyzed.


# Can I get a copy of the source?
Sure, here's a lynx [https://github.com/IdiomaticLynx/bd-region-check]

The code is based on a heavily modifed [libbluary](https://www.videolan.org/developers/libbluray.html) and therefore falls under the GPLv2.1 or later license
If you decide to use this code in any manner, there's no need to credit me, but do credit the libbluray developers.
