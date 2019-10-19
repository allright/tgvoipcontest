#!/usr/bin/env python
import requests
import subprocess
import json


def write_to_file(resp, filename):
    f = open(filename, 'w')
    f.write(resp)
    f.close()


def get_config_for_call(call_id):
    url = "https://api.contest.com/voip677067:77f2c58ea74619d0a8115755ed7fcd62682a/getConnection?call=%s" % call_id
    return requests.get(url)


response = get_config_for_call('1232131234')
response = response.json()
print "get_config_for_call: url %s" % response

if not response['ok']:
    print "malformed response"
    exit(-1)

result = response['result']
config = result['config']
write_to_file(json.dumps(config), 'config.json')
endpoints = result['endpoints']


endpoint = endpoints[0]
reflector = endpoint['ip']
port = endpoint['port']

peer_tags = endpoint['peer_tags']
tag_caller_hex = peer_tags['caller']
tag_callee_hex = peer_tags['callee']

print reflector
print port
print tag_caller_hex
print tag_callee_hex

subprocess.call(["ls", "-l"])