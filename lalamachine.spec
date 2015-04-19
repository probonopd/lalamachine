#
# spec file for package lalamachine
#
# Copyright (c) 2015 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name: lalamachine
Version: 0.13.0
Release: 1
License: GPL-3.0+
Summary: A music player that is awesome
Url: https://bitbucket.org/r-mean/lalamachine
Group: Applications/Multimedia
Source: %_sourcedir/%{name}-%{version}.tar.gz
Requires: libqt5-qtquickcontrols >= 5.4.1, libQt5Multimedia5 >= 5.4.1
BuildRequires: libqt5-qtbase-devel >= 5.4.1, libqt5-qtmultimedia-devel >= 5.4.1 libtag-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
Lalamachine is a music player.

%prep
%setup -q

%build
qmake-qt5 "target.path=%{buildroot}/usr/local/bin" \
      "icon.path=%{buildroot}/usr/share/icons/hicolor/64x64/apps" \
      "desktop.path=%{buildroot}/usr/share/applications" \
      lalamachine.pro
make %{?_smp_mflags}

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/local/bin/lalamachine
/usr/share/icons/hicolor/64x64/apps/lalamachine.png
/usr/share/applications/lalamachine.desktop

%changelog
* Sun Apr 19 2015 r-mean
  0.12.0-1: Release 0.12.1
  * Automatic Playlists.
  * The total playtime is always calculated, also for the library.

* Sat Apr 04 2015 r-mean
  0.11.0-1: Release 0.11.1
  * Playlists now have a name when loaded.
  * Changing a playlist will automatically save the changes.
  * The app remembers which playlist was loaded between sessions.
  * Playlists can now be named (save as) in the menu.
  * Removed the save button from the button bar.

* Fri Apr 03 2015 r-mean
  0.10.1-1: Release 0.10.1
  * Fixed bug that could crash the application while scanning the database.
  * Added Info dialog.
  * Double clicking a library track is now playing it instead o adding it to
    the current playlist.

* Wed Apr 01 2015 r-mean
  0.10.0-1: Release 0.10
  * The musiclib is now a sqlite database.

* Sun Mar 29 2015 r-mean
  0.9.0-1: Release 0.9
  * If a comment starts with a camelot key it is sorted from 1-12.
  * Track numbers are now sorted correctly again.
  * Added a right click menu to add or remove tracks from playlists.
  * Moved playlist open, delete and burn into the menu bar of the main window.

* Sun Mar 15 2015 r-mean
  0.8.0-1: Release 0.8
  * Sorting a playlist is now significantly faster. The whole library can now
    be sorted.
  * Each table displays now how many entries it has.

* Sun Mar 15 2015 r-mean
  0.7.0-1: Release 0.7
  * The library can now be rescanned.
  * The library path is now configurable.
  * Fixed a bug that would not update the artist and album list after the
    library has changed.

* Sat Mar 14 2015 r-mean
  0.6.0-1: Release 0.6
  * Fixed a bug that would play the wrong next track if the playlist was
    reordered.
  * Fixed a bug that would not save the musiclib in UTF8.
  * The misc playlist is no longer visible/selectable in the playlistlist.

* Sun Mar 08 2015 r-mean
  0.5.0-1: Release 0.5
  * The window tile now contains the title of the currently playing track.
  * Playlists can now be deleted.
  * Playlists can now be burned as audio cd with K3b.

* Wed Mar 04 2015 r-mean
  0.4.0-1: Release 0.4
  * Limiting the size of the playlist for sorting.
  * The filter lists are alphabetically sorted and can no longer be sorted
    manually.
  * Fixed a bug that would highlight the wrong track when the same track is in
    in the list more than once. (UUID for each playlist_model entry)

* Sun Mar 01 2015 r-mean
  0.3.0-1: Added a rhythmbox like music library.

* Sat Feb 07 2015 r-mean
  0.2.4-1: Testrelease to see if the preun scriptlet now works.

* Sat Feb 07 2015 r-mean
  0.2.3-1: Fixed the condition done should be fi.

* Sat Feb 07 2015 r-mean
  0.2.2-1: Changed the removal of the sumlink from postun to preun.

* Sat Feb 07 2015 r-mean
  0.2.1-1: Fixed the postun scriptlet in the spec file to not remove the
  symlink on upgrade.

* Sat Feb 07 2015 r-mean
  0.2.0-1: Release 0.2
  * Added keyboard control.
  * Playlists can now be saved and loaded.
  * The total playtime of the playlist is now displayed.
  * App settings can now be stored in a JSON file.
  * The last volume is remembered on app start.

* Sat Feb 07 2015 r-mean
  0.1.0-1: Release 0.1 - A useable player.

* Sat Feb 07 2015 r-mean
  0.0.9-1: Initial package.
