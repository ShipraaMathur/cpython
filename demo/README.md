# Usage

```bash
export PYTHONSTARTUP=hello.py
python
```

Then in Python..

```python
import demo_audit

import bad_module
bad_module.load_config('good.yaml')
bad_module.load_config('bad.yaml')
```
Should catch the attempt to open a URL
