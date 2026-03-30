from setuptools import find_packages, setup

package_name = 'my_py_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='luke',
    maintainer_email='luke@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            "test_node = my_py_pkg.test_node:main",
            "uwb_driver = my_py_pkg.uwb_driver:main",
            "target_follower = my_py_pkg.target_follower:main",
            "local_planner_tester = my_py_pkg.local_planner_tester:main",
            "arduino_serial_manager = my_py_pkg.arduino_serial_manager:main"
        ],
    },
)
