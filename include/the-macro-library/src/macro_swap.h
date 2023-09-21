/*
Copyright 2023 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _macro_swap_H
#define _macro_swap_H

/*
    macro_swap requires tmp to be declared and to be of the type that
    a and b are pointed to.
*/
#define macro_swap(a, b)    \
    tmp = *(a);             \
    *(a) = *(b);            \
    *(b) = tmp

#endif /* _macro_swap_H */