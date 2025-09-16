#
# spec file for package mqttsleep
#
# Copyright (c) 2025 SUSE LLC
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           mqttsleep
Version:        1
Release:        1
Summary:        Sleep until awakened by MQTT signal
License:        MIT
URL:            https://github.com/oli4vr/mqttsleep
Packager:       Olivier Van Rompuy <oli4vr@gmail.com>
BuildRoot:      %{_tmppath}/%{name}-build
Requires:       libmosquitto1

%description
Sleep until awakened by MQTT signal

%prep
rm -rf %{buildroot}
cp %{_sourcedir}/mqttsleep .
cp %{_sourcedir}/mqttwake .

%build
# Nothing to build

%install
mkdir -p %{buildroot}/usr/bin
install -m 0755 mqttsleep %{buildroot}/usr/bin/
install -m 0755 mqttwake %{buildroot}/usr/bin/

%clean
rm -rf %{buildroot}

%files
/usr/bin/mqttsleep
/usr/bin/mqttwake

