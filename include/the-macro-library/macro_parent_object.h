// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_parent_object_H
#define _macro_parent_object_H

#include <stddef.h>

/*
  Given an address of a member of a structure, the base object type, and the
  field name, return the address of the base structure.
*/
#ifndef macro_parent_object
#define macro_parent_object(addr, base_type, field)    \
  ((base_type *)((char *)(addr) - offsetof(base_type, field)))
#endif

#endif
