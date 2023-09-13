# Copyright 2023 Andy Curtis
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys

def indent_macro_properly(file_contents):
    """
        This scans file_content for line continuations and then lines them up within
        each group.
    """

    # Flags to track whether we are inside a comment or inside quotes
    in_comment = False
    in_quote = False

    # List to hold individual characters for the resulting stripped code
    output = []

    # Initialize loop variables
    i = 0
    n = len(file_contents)

    # Loop through each character in the original code
    while i < n:
        if not in_comment and not in_quote:
            # Detect single-line comment '//'
            if file_contents[i:i + 2] == "//":
                ni = file_contents.find("\n", i) + 1
                if ni == 0:
                    output.append(file_contents[i:])
                    break  # End of file, exit loop
                else:
                    output.append(file_contents[i:ni])
                    i = ni

            # Detect multi-line comment '/*'
            elif file_contents[i:i + 2] == "/*":
                in_comment = True
                output.append(file_contents[i:i + 2])
                i += 2  # Skip '/*'

            # Detect quotes '"'
            elif file_contents[i] == '"':
                in_quote = True
                output.append('"')
                i += 1  # Skip '"'

            elif file_contents[i:i + 2] == '\\\n':
                # get the current line by scanning the output backwards
                current_output = ''.join(output)
                idx = current_output.rfind('\n')
                current_line = current_output[idx+1:]
                current_output = current_output[:idx+1]
                output = [current_output]
                rest_of_file = current_line + file_contents[i:]
                lines = rest_of_file.split('\n')
                group = []

                for line_no in range(0, len(lines)):
                    if len(lines[line_no]) > 0 and lines[line_no][-1] == '\\':
                        group.append(lines[line_no][0:-1].rstrip())
                    else:
                        break

                max_length = 0
                for line in group:
                    if len(line) > max_length:
                        max_length = len(line)

                max_length = max_length + 4
                for line in group:
                    count = len(line)
                    output.append(line +  (' ' * (max_length-count)) + '\\\n')

                lines = lines[len(group):]
                file_contents = '\n'.join(lines)
                i = 0
                n = len(file_contents)

            # Add all other characters to output
            else:
                output.append(file_contents[i])
                i += 1

        elif in_comment:
            # End of multi-line comment '*/'
            if file_contents[i:i + 2] == "*/":
                in_comment = False
                output.append(file_contents[i:i + 2])
                i += 2  # Skip '*/'

            # Move to the next character
            else:
                output.append(file_contents[i])
                i += 1

        elif in_quote:
            # End of quotes '"'
            if file_contents[i] == '"':
                in_quote = False

            # Add the character within quotes to output
            output.append(file_contents[i])
            i += 1  # Move to the next character

    # Combine list into a single string
    return ''.join(output)



def process_macro_file(filename):
    # Open the file and read its contents
    with open(filename, 'r', encoding='utf-8') as file:
        file_contents = file.read()

    new_file_contents = indent_macro_properly(file_contents)
    with open(filename, 'w', encoding='utf-8') as file:
        file.write(new_file_contents)

for idx in range(1, len(sys.argv)):
    process_macro_file(sys.argv[idx])
