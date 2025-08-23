#!/usr/bin/env python3

# Copyright 2019-2025 Andy Curtis
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

def macrofy(filename, start, end):
    with open(filename, 'r', encoding='utf-8') as file:
        file_contents = file.read()


    lines = file_contents.split('\n')

    max_length = 0
    for idx in range(start, end):
        if idx >= len(lines):
            continue
        line = lines[idx]
        if len(line) == 0:
            continue
        if line[-1] == '\\':
            line = line[0:-1].rstrip()
            lines[idx] = line

        if len(line) > max_length:
            max_length = len(line)

    max_length = max_length + 4
    for idx in range(start, end):
        if idx >= len(lines):
            continue

        count = len(lines[idx])
        lines[idx] = lines[idx] + (' ' * (max_length-count)) + '\\'

    print('\n'.join(lines))

start = int(sys.argv[2])
end = int(sys.argv[3])

macrofy(sys.argv[1], start, end)
