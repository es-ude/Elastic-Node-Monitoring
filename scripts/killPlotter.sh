#!/bin/bash
kill -9 $(ps aux | grep '[p]ython ./icac_demo_runner.py' | awk '{print $2}')
