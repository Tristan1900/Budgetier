# Disk Cache Admission Control Experiment
This is a research experiment for NVMe disk cache system with a background of CDN.

NVMe is an interface specification developed specifically for NAND flash and next-generation SSDs, it is super fast both in read and write compared to SATA interface. But the problem with NAND flash is that it has limited amount of writes during its life time. For a samsung SSD(960 EVO NVMe M.2 250GB) that we use for experiment, it has 150TB write budget according to Smartmontools.

The goal of this research is
* to find a good control policy that can limit the object admission to disk cache
* to maintain high hit ratios 
* to be able to handle real request pattern in CDN


## Experiment explianed
The experiment is based on [Varnish Cache](https://github.com/varnishcache/varnish-cache), the backend server is using [origin](https://github.com/dasebe/webtracereplay) with Nginx.

### Workflow
![alt Experiment workflow](./asset/CDN.png)

The workflow is showed as above. This is only an illustrative diagram of how our experiment works, the specific internal workflow of varnish is linked [here](https://book.varnish-software.com/4.0/chapters/VCL_Basics.html).

The basic idea is when a cache miss happens, varnish will call our customized Vmod, in which we can implement our admission policy to decide whether to admit that particular object into the cache. We set varnish to use disk as its cache by using this flag
```
-s Memory=file,/your/path/to/disk,200g
``` 
varnish then will use mmap to map disk to memory.

### Control policy 
To control obejct admission, the intuitive way is not to admit those object that only appear few times. One way to do that is maintaining a ghost cache beside the Varnish Cache(real cache). The ghost cache is a least recent use cache that keeps track of many objects(2x size of real cache) by recording their metadata and keeping counters. When a request comes in and has cache miss, varnish will call our vmod and put the metadata of that object into ghost cache and set the counter of that object to be one. Next time if the same object comes, its counter will increase. We keep a threshold that only object with a greater counter value can be admitted into real cache. Clearly by doing this we can admit objects that potentially lead to high hit ratio.

The way to tune the threshold is similar to [Pannier](https://dl.acm.org/citation.cfm?id=3094785). We calculate a quota for a time interval. The quota is the amount of writes that allowed, and the way to calculate, for example, is to divide 150TB by 3 years. Below that quota in a given time interval, we admit everyhing. When amount of writes is over that quota, we begin to control admission by increasing threshold. And the penalty for excreeding quota is to not admit anything in next few time interval until the average of writes comes below the quota again. The detailed explanition is in that paper.

Another way to control admission is to use a probability model. Instead of setting threshold and maintaing ghost cache, we tune a probability of admission using the same idea mentioned above. 


## Getting Started



### Prerequisites

To build varnish from scratch, we first need to install [dependencies](https://varnish-cache.org/docs/trunk/installation/install.html)

For ubuntu that we use, install the following

```
sudo apt-get install \
    make \
    automake \
    autotools-dev \
    libedit-dev \
    libjemalloc-dev \
    libncurses-dev \
    libpcre3-dev \
    libtool \
    pkg-config \
    python-docutils \
    python-sphinx
```

### Get Varnish source code

We use latest Varnish version 6.0 from github.

```
git clone https://github.com/varnishcache/varnish-cache.git
```
And checkout to 6.0 branch

### Install Varnish

Just like install any other software

```
cd varnish-cache
./configure --prefix=/usr/local/varnish/
make 
make install
```

### Install Vmod



## Acknowledgments

* [Daniel S. Berger](https://github.com/dasebe)