#put Spelunky.exe.h with the Entity structs and execute the script to get the classes
import re

replace = [
	["undefined8", "long long"],
	["undefined6", "long long"],
	["undefined4", "int"],
	["undefined2", "short"],
	["undefined1", "char"],
	["undefined", "char"],
	["byte", "char"],
	["?", "_maybe"],
	["uint32_t", "int"],
	["int32_t", "int"],
	["enum ", ""],
	["hitbox_y_up", "hitbox_up"],
    ["hitbox_y_down", "hitbox_down"],
    ["Ownership ownership", "Ownership owner"],
	["\n    undefined * * vftable;", ""],
	["\n    struct EntityVFTable * vftable;", ""],
	["\n    struct EntityActiveVFTable * vftable;", ""],
]
#entityClassRe = r"struct Entity([^V]\w*) \{\n[\s\S]*?\}"
removeBaseRe = re.compile(r"struct (?:Base)?(Entity\w*) \{\n\s*struct Base(Entity\w*) \w*;([\s\S]+?)\}")
baseEntityRe = re.compile(r"\nstruct Base(Entity\w*) \{([\s\S]+?)\}")
#baseEntityInRe = re.compile(r"    (struct BaseEntity\w* \{[\s\S]+?\})")
import sys

sys.stdout = open("classes.h", "w")
baseEntities = {}
with open('spelunky.exe.h', encoding="utf-8") as f:
	code = f.read()

for i in replace:
	code = code.replace(i[0], i[1])

params_iterator = baseEntityRe.finditer(code)
for m in params_iterator:
	baseEntities[m[1]] = m[2];

params_iterator = removeBaseRe.finditer(code)
out = ""
for m in params_iterator:
	baseContent = baseEntities[m[2]]
	if m[1] == "EntityActive":
		i = 0
	if baseContent:
		if m[1] == m[2]:
			if m[1] == "EntityActive":
				continue
			out += f"struct {m[1]} {{\n{baseContent}{m[3]}}};\n"
		else:
			out += f"struct {m[1]} : public {m[2]} {{{m[3]}}};\n"

replace = [
	["EntityType entity_type;", "uint32_t entity_type;"],
	["struct Entity {\n", "struct Entity {\n    void * * vfTable;"],
	["struct", "class"],
	["\n\n};", "\n\n    void PlaySound(const char *audioName);\n};"],
]
for i in replace:
	out = out.replace(i[0], i[1])
#out = "#pragma once\n" + out
out = re.sub(r"(class .*\{)", r"\1\npublic:", out)
print(out)