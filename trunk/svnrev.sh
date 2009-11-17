#! /bin/bash
#
rev_new_raw=$(svnversion -n . 2>/dev/null | tr '\n' ' ' | tr -d '\r')
[ -n "$rev_new_raw" ] || rev_new_raw=$(SubWCRev . 2>/dev/null | tr '\n' ' ' | tr -d '\r')


rev_new_raw=$(echo $rev_new_raw | sed 's/[^0-9]*\([0-9]*\)\(.*\)/\1 \2/')
rev_new=0
a=$(echo $rev_new_raw | sed 's/\([0-9]*\).*/\1/')
let "a+=0"
#find max rev
while [ "$a" ]; do
	[ "$a" -gt "$rev_new" ] && rev_new=$a
	rev_new_raw=$(echo -n $rev_new_raw | sed 's/[0-9]*[^0-9]*\([0-9]*\)\(.*\)/\1 \2/')
	a=$(echo $rev_new_raw | sed 's/\([0-9]*\).*/\1/') 
done

rev_old=$(cat ./source/svnrev.c 2>/dev/null | tr -d '\n' | sed 's/[^0-9]*\([0-9]*\).*/\1/')

if [ "$rev_new" != "$rev_old" ] || [ ! -f ./source/svnrev.c ]; then
	
	cat <<EOF > ./source/svnrev.c
#define SVN_REV "$rev_new"

const char *GetRev()
{
	return SVN_REV;
}
EOF

	if [ -n "$rev_new" ]; then
		echo "Changed Rev $rev_old to $rev_new" >&2
	else
		echo "svnrev.c created" >&2
	fi
	echo >&2
	
	cat <<EOF > ./HBC/META.XML
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<app version="1">
		<name> USB Loader GX</name>
		<coder>USB Loader GX Team</coder>
		<version>1.0 r$rev_new</version>
		<release_date>200910150000</release_date>
		<short_description>Loads games from USB-devices</short_description>
		<long_description>USB Loader GX is a libwiigui based USB iso loader with a wii-like GUI. You can install games to your HDDs and boot them with shorter loading times.
The interactive GUI is completely controllable with WiiMote, Classic Controller or GC Controller.
Features are automatic widescreen detection, coverdownload, parental control, theme support and many more.

Credits:
Coding: Dimok, nIxx, giantpune, ardi, Hungyip84, DrayX7, Lustar, r-win, WiiShizzza
Artworks: cyrex, NeoRame
WiiTDB / Hosting covers: Lustar
Hosting updates files: CorneliousJD
USBLoader sources: Waninkoko, Kwiirk, Hermes
Languages files updates: Kinyo and translaters
Hosting themes: Deak Phreak

Libwiigui: Tantric
Libogc/Devkit: Shagkur and Wintermute
FreeTypeGX: Armin Tamzarian.

Links:
USB Loader GX Project Page and Support Site:
http://code.google.com/p/usbloader-gui/
Help Website:
http://usbloadergx.koureio.net/
WiiTDB Site:
http://wiitdb.com
Themes Site:
http://wii.spiffy360.com
Languages Translaters Page:
http://gbatemp.net/index.php?showtopic=155252

Libwiigui Website:
http://wiibrew.org/wiki/Libwiigui/
FreeTypeGX Project Page:
http://code.google.com/p/freetypegx/
Gettext Official Page:
http://www.gnu.org/software/gettext/gettext.html
		</long_description>
	</app>
EOF

fi
echo $a
