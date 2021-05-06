import yaml


def parse_yaml_config(yaml_file):
    with open(yaml_file, 'r') as fp:
        config = yaml.load(fp, Loader=yaml.FullLoader)
    return config
