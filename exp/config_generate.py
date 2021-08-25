import configparser
import argparse

parser = argparse.ArgumentParser(description='server config generate')
parser.add_argument(
    '-server_name',
    dest='server_name',
    type=str,
    help='server_name',
)
parser.add_argument(
    '-graph_define_directory',
    dest='graph_define_directory',
    type=str,
    help='graph_define_directory',
)

parser.add_argument(
    '-log_directory',
    dest='log_directory',
    type=str,
    help='log_directory',
)
parser.add_argument(
    '-result_directory',
    dest='result_directory',
    type=str,
    help='result_directory',
)
parser.add_argument(
    '-config_path',
    dest="config_path",
    type=str,
    help="config path"

)

args = parser.parse_args()

conf = configparser.ConfigParser()

conf.add_section('Main') #添加section
conf.set('Main', 'server_name', args.server_name)

conf.add_section('Path') #添加section
conf.set('Path', 'graph_define_directory', args.graph_define_directory)
conf.set('Path', 'log_directory', args.log_directory)
conf.set('Path', 'result_directory', args.result_directory)

conf.add_section('LOG') #添加section
conf.set('LOG', 'log_level', "INFO")

conf.add_section('Input') #添加section
conf.set('Input', 'read_edge_count', "-1")
conf.set('Input', 'result_type', "visited_count")

with open(args.config_path + ".ini", 'w') as fw:
    conf.write(fw)