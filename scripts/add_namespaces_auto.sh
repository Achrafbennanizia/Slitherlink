#!/bin/bash
# Add namespaces to all remaining header and source files

cd /Users/achraf/CLionProjects/Slitherlink

# Function to add namespace to a header file
add_namespace_to_header() {
    file=$1
    if ! grep -q "namespace slitherlink" "$file"; then
        # Find the line number after #pragma once and includes
        temp_file=$(mktemp)
        awk '
            BEGIN { in_includes = 0; last_include = 0 }
            /#pragma once/ { print; in_includes = 1; next }
            in_includes && /#include/ { print; last_include = NR; next }
            in_includes && !/^#include/ && !/^$/ && !namespace_added { 
                print ""
                print "namespace slitherlink {"
                print ""
                namespace_added = 1
            }
            { print }
            END {
                if (!namespace_added) {
                    print ""
                    print "} // namespace slitherlink"
                } else {
                    print ""
                    print "} // namespace slitherlink"
                }
            }
        ' "$file" > "$temp_file"
        mv "$temp_file" "$file"
        echo "Added namespace to $file"
    fi
}

# Add namespaces to remaining header files
for file in include/slitherlink/solver/*.h include/slitherlink/io/*.h include/slitherlink/factory/*.h; do
    if [ -f "$file" ] && ! grep -q "namespace slitherlink" "$file"; then
        echo "Processing $file..."
        add_namespace_to_header "$file"
    fi
done

echo "Namespace addition complete!"
