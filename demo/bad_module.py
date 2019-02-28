import yaml

def load_config(f):
    with open(f, 'r') as f_h:
        cfg = yaml.load(f_h)
    return cfg