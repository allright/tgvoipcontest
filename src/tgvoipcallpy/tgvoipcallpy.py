#!/usr/bin/env python
import requests
import subprocess
import json
import random
import sys
from threading import Thread
import time

auth_token = '677067:77f2c58ea74619d0a8115755ed7fcd62682a'

def write_to_file(resp, filename):
    f = open(filename, 'w')
    f.write(resp)
    f.close()


def get_config_for_call(call_id):
    url = "https://api.contest.com/voip%s/getConnection?call=alrightid%s" % (auth_token,call_id)
#    print url
    return requests.get(url)


def output_reader(proc, outq):
    for line in iter(proc.stdout.readline, b''):
        outq.put(line.decode('utf-8'))


print(sys.argv)


random_call_id = random.randint(1,10000000000000000)
response = get_config_for_call(random_call_id)
response = response.json()
print "get_config_for_call: url %s" % response

if not response['ok']:
    print "malformed response"
    exit(-1)

result = response['result']
config = result['config']
write_to_file(json.dumps(config), 'config.json')

encryption_key_hex = result['encryption_key']
endpoints = result['endpoints']


endpoint = endpoints[0]
reflector = endpoint['ip']
port = endpoint['port']
id = endpoint['id']

peer_tags = endpoint['peer_tags']
tag_caller_hex = peer_tags['caller']
tag_callee_hex = peer_tags['callee']

app = sys.argv[1]
soundA = sys.argv[2]
sound_output_B = sys.argv[3]
soundB = sys.argv[4]
sound_output_A = sys.argv[5]
rate_app = sys.argv[6]

# print reflector
# print port
# print tag_caller_hex
# print tag_callee_hex
# print encryption_key_hex
#call = [app, "in.wav", "oo.wav"]
caller = [app,
        "%s:%s" % (reflector,port),
        tag_caller_hex,
        "-k", encryption_key_hex,
        "-i", soundA,
        "-o", sound_output_B,
        "-c", "config.json",
        "-r", "caller"]
      #  "-id", id]

callee = [app,
          "%s:%s" % (reflector,port),
          tag_callee_hex,
          "-k", encryption_key_hex,
          "-i", soundB,
          "-o", sound_output_A,
          "-c", "config.json",
          "-r", "callee"]
       #   "-id", id]

for c in caller:
    sys.stdout.write("%s "%c)
print

for c in callee:
    sys.stdout.write("%s "%c)
print

def call_func(arg):
    proc_name = arg[12]
    subprocess.call(arg)

caller_thread = Thread(target = call_func,args=(caller,))
callee_thread = Thread(target = call_func,args=(callee,))

caller_thread.start()
callee_thread.start()

caller_thread.join()
callee_thread.join()

subprocess.call([rate_app,soundA,sound_output_A])
subprocess.call([rate_app,soundB,sound_output_B])