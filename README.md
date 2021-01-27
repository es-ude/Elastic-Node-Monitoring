# Elastic Node Monitoring

This project provides the funcionallity to measure the power consumption on the [elastic node](https://ieeexplore.ieee.org/document/8831207)
as addition to the [elastic node middleware](https://github.com/es-ude/ElasticNodeMiddleware).

## Use the Monitoring Code

For the complete guide please refer to the [EnergyMonitoring.md](https://github.com/es-ude/ElasticNodeMiddleware/blob/master/docs/CloneGuide.md) in the middleware repository.

## Quick Start Guide

### Setup

You need to set the serial port to your elastic node and programmer and the path to your data folder in the [scripts/config.py](scripts/config.py) and the port to your pogrammer again in the [user.bazelrc](user.bazelrc).

### Upload Code

    $ bazel run //app:measurement_upload --platforms=@AvrToolchain//platforms:ElasticNode_v4_monitor

### Capture Measurment

    bazel run captureMeasurement capture <filename>
  
