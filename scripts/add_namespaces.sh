#!/bin/bash

# Script to add namespace wrappers to header files

# Add namespace to State.h
sed -i.bak '3a\
namespace slitherlink {
' include/slitherlink/core/State.h

echo -e '\n} // namespace slitherlink' >> include/slitherlink/core/State.h

# Add namespace to Solution.h
sed -i.bak '4a\
namespace slitherlink {
' include/slitherlink/core/Solution.h

echo -e '\n} // namespace slitherlink' >> include/slitherlink/core/Solution.h

# Clean up backup files
rm -f include/slitherlink/core/*.bak

echo "Namespace wrappers added successfully"
