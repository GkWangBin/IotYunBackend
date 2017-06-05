CREATE TABLE `dev_t` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `devid`   int(10) NOT NULL COMMENT '设备唯一标识符', 
  `devname` varchar(64) NOT NULL COMMENT '设备名字',
  `sceneid` int(10) NOT NULL COMMENT '归属场景',
  `devtype` int(5) DEFAULT '0' COMMENT '设备类型',
  `owner` varchar(127) DEFAULT NULL COMMENT '制造商',
  `phone` char(11) DEFAULT NULL COMMENT '联系方式',
  `ctime` int(10) DEFAULT '0' COMMENT  '安装时间',
  `flag` tinyint(1) DEFAULT '0' COMMENT '是否有效',
  PRIMARY KEY (`id`),
  KEY `devid` (`devid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT '设备表';

CREATE TABLE `info_t_0` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `devid`   int(10) NOT NULL COMMENT '设备唯一标识符', 
  `info` varchar(255) DEFAULT NULL COMMENT '采集到的信息',
  `ctime` int(10) DEFAULT '0' COMMENT  '采集时间',
  `flag` tinyint(1) DEFAULT '0' COMMENT '是否有效',
  PRIMARY KEY (`id`),
  KEY `devid` (`devid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT '信息表';

select devid from dev_t where devid=%s and flag=1;

insert into info_t_%d (devid, info, ctime, flag) values (%s, \"%s\", %d, %d);

UPDATE dev_t SET flag=0 WHERE devid=%s;
UPDATE info_t_0 SET flag=0 WHERE devid=%s;