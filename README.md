# The Disk Cache Tuning Experiment
This is a research experiment for NVMe disk cache system with a background of CDN disk-based cache.

NVMe is an interface specification developed specifically for NAND flash and next-generation SSDs, it is super fast both in read and write compared to SATA interface. But the problem with NAND flash is that it has certain amount of writes during its life time. For a samsung SSD(960 EVO NVMe M.2 250GB) that we use for experiment, it has 150TB write budget according to Smartmontools.

The goal of this research is
* to find a good control policy that can limit the object admission to disk cache
* to maintain high hit ratios 
* to be able to handle real request pattern in CDN


## Experiment explianed
The experiment is based on [Varnish Cache](https://github.com/varnishcache/varnish-cache), the backend server is using [origin](https://github.com/dasebe/webtracereplay) with Nginx.

### Workflow
![alt Experiment workflow](./asset/CDN.png)
The workflow is showed as above. This is only an illustrative diagram of how our experiment works, the specific internal workflow of varnish is linked [here].(https://book.varnish-software.com/4.0/chapters/VCL_Basics.html)
The basic idea is when a cache miss happens, varnish will call our customized Vmod, in which we can implement our admission policy to decide whether to admit that particular object into the cache. We set varnish to use disk as its cache by using this flag
```
-s Memory=file,/your/path/to/disk,200g
``` 
varnish then will use mmap to map disk to memory.



## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them

```
Give examples
```

### Installing

A step by step series of examples that tell you have to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone who's code was used
* Inspiration
* etc