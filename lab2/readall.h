/**
 * author wukat
*/
ssize_t readall(int fd, const void *buf, size_t nbyte) {
	ssize_t nread = 0, n;

	do {
		if ((n = read(fd, &((char *)buf)[nread], nbyte - nread)) == -1) {
			if (errno == EINTR) /*EINTR when signal interrupted before saved*/
				continue;
			else
				return -1;
		}
		if (n == 0) /*case when EOF was read*/ {
			return nread;
		}
		nread += n;
	} while (nread < nbyte);
	return nread;
}
