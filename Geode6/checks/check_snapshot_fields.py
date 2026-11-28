from pathlib import Path
import re
from collections import Counter
root = Path(__file__).resolve().parents[1]
bindings = Path(__import__('sys').argv[1])
classes = ['PlayerCheckpoint', 'GJGameState', 'EffectManagerState', 'DynamicObjectAction', 'KeyframeObject']
for cls in classes:
    fields = re.findall(r'^    [\w:<> ,*&]+\s+(\w+);', (bindings / (cls + '.hpp')).read_text(), re.M)
    src = (root / 'PersistenceAPI/src/hooks' / (cls + '.cpp')).read_text()
    read_start = src.index('inline void persistenceAPI::operator>>')
    write_start = src.index('inline void persistenceAPI::operator<<')
    reads = re.findall(r'i_stream >> o_value\.(\w+);', src[read_start:write_start])
    writes = re.findall(r'o_stream << i_value\.(\w+);', src[write_start:src.find('#if defined', write_start) if '#if defined' in src[write_start:] else len(src)])
    missing = [f for f in fields if not re.search(r'\bo_value\.' + f + r'\b', src[read_start:write_start])]
    duplicate = [f for f, n in Counter(reads).items() if n > 1]
    assert not missing, (cls, 'unsaved native fields', missing)
    assert not duplicate, (cls, 'duplicate native fields', duplicate)
    assert reads == writes, (cls, 'read/write field order differs')
    print(cls + ': all native fields covered; read/write order matches')
