# Patches for Ndless integration (experimental)

These are patches for Ndless to integrate the bFLT loader into Ndless itself.

Warning: experimental!

## Using these patches

Ensure your ```ploaderhook.c``` and ```Makefile``` files in ```/path/to/ndless_src/trunk/arm/``` match the patch revision.

Get a copy of the bFLT loader source code and copy everything in the ```bflt/``` folder to your ```/path/to/ndless_src/trunk/arm/``` folder.

Run ```patch -p0 -i /path/to/patch``` in your ```/path/to/ndless_src/trunk/``` folder to apply the patches.

Run ```make``` to recompile Ndless.

On your calculator, uninstall Ndless by clicking on ```ndless_resources``` on your calculator if you have Ndless on the calculator already. Copy the new ```ndless_resources``` in your ```/path/to/ndless_src/trunk/calcbin/``` folder and send the new OS located in that folder.

You should now have bFLT loading built into Ndless.