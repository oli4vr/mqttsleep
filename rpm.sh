#!/bin/bash
rm -rf rpmbuild 2>/dev/null
rpmbuild --define "_sourcedir $(pwd)" --define "_topdir $(pwd)/rpmbuild" -bb mqttsleep.spec
mv rpmbuild/RPMS/x86_64/mqttsleep-*.rpm .
rm -rf rpmbuild 2>/dev/null
