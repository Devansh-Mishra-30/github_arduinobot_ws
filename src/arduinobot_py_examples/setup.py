import os
from glob import glob
from setuptools import find_packages, setup

package_name = 'arduinobot_py_examples'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob(os.path.join('launch', '*launch.[pxy][yma]*')))
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='devansh',
    maintainer_email='devansh@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'simple_publisher = arduinobot_py_examples.simple_publisher:main',
            'simple_subscriber = arduinobot_py_examples.simple_subscriber:main',
            'simple_parameter = arduinobot_py_examples.simple_parameter:main',
            'simple_service_server = arduinobot_py_examples.simple_service_server:main',
            'simple_service_client = arduinobot_py_examples.simple_service_client:main',
            'simple_action_server = arduinobot_py_examples.simple_action_server:main',
            'simple_action_client = arduinobot_py_examples.simple_action_client:main',
            'simple_moveit_interface = arduinobot_py_examples.simple_moveit_interface:main',
            'simple_lifecycle_node= arduinobot_py_examples.simple_lifecycle_node:main'

],
    },
)
