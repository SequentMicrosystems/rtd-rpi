import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="librtd",
    version="1.1.0",
    author="Sequent Microsystems",
    author_email="olcitu@gmail.com",
    zip_safe=True,
    description="A set of functions to control Sequent Microsystems MEGA_RTD board",
	license='MIT',
    url="https://www.sequentmicrosystems.com",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 2/3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
)
